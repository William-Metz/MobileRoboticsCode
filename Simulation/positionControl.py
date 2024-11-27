#!/opt/homebrew/Caskroom/miniforge/base/envs/visualize/bin/python
# You probably want to change the above line to: #!/usr/bin/env python

from __future__ import annotations

from argparse import ArgumentParser
from dataclasses import dataclass
from math import atan2, cos, pi, sin

import matplotlib
import matplotlib.pyplot as plt

matplotlib.use("TkAgg")


def wrap_to_pi(angle: float) -> float:
    return (angle + pi) % (2 * pi) - pi


@dataclass
class Pose:
    x: float = 0
    y: float = 0
    theta: float = 0

    def __str__(self) -> str:
        return f"{self.x:.3f} {self.y:.3f} {self.theta:.3f}"

    def copy(self) -> Pose:
        return Pose(self.x, self.y, self.theta)


class ForwardKinematics:
    def __init__(self, track_width: float):
        self.track_width = track_width
        self.pose = Pose()

    def update(self, left_velocity: float, right_velocity: float, dt: float) -> Pose:
        v = (right_velocity + left_velocity) / 2
        theta_dot = (right_velocity - left_velocity) / self.track_width

        self.pose.x += v * cos(self.pose.theta) * dt
        self.pose.y += v * sin(self.pose.theta) * dt
        self.pose.theta = wrap_to_pi(self.pose.theta + theta_dot * dt)

        return self.pose


class GoalPositionControl:
    def __init__(
        self,
        goal_x: float,
        goal_y: float,
        max_velocity_angular: float,
        max_velocity_linear: float,
        track_width: float,
        gain_orientation: float,
        gain_position: float,
    ):
        self.goal_x = goal_x
        self.goal_y = goal_y

        self.max_velocity_angular = max_velocity_angular
        self.max_velocity_linear = max_velocity_linear

        self.track_width = track_width

        self.gain_orientation = gain_orientation
        self.gain_position = gain_position

    def update(self, pose: Pose, threshold: float) -> tuple[float, float]:
        dx = self.goal_x - pose.x
        dy = self.goal_y - pose.y

        d_error = ((dx * dx) + (dy * dy)) ** 0.5

        if d_error < threshold:
            return 0, 0

        v = min(self.gain_position * d_error, self.max_velocity_linear)

        theta_to_goal = atan2(dy, dx)
        theta_error = theta_to_goal - pose.theta
        theta_dot = min(self.gain_orientation * theta_error, self.max_velocity_angular)
        theta_dot = max(theta_dot, -self.max_velocity_angular)

        left_velocity = v - theta_dot * self.track_width / 2
        right_velocity = v + theta_dot * self.track_width / 2

        return left_velocity, right_velocity


def simulate(
    duration: float,
    time_step: float,
    track_width: float,
    goal_x: float,
    goal_y: float,
    goal_threshold: float,
    max_velocity_angular: float,
    max_velocity_linear: float,
    K_orientation: float,
    K_position: float,
) -> list[Pose]:
    time = 0

    forward_kinematics = ForwardKinematics(track_width)
    position_control = GoalPositionControl(
        goal_x,
        goal_y,
        max_velocity_angular,
        max_velocity_linear,
        track_width,
        K_orientation,
        K_position,
    )

    v_left = 0
    v_right = 0

    poses = [forward_kinematics.pose.copy()]

    # Numerical simulation loop
    while time < duration:
        # Update the pose based on kinematics
        pose = forward_kinematics.update(v_left, v_right, time_step)
        poses.append(pose.copy())

        # Update position control based on pose
        v_left, v_right = position_control.update(pose, goal_threshold)

        time += time_step

    return poses


def main():
    parser = ArgumentParser("Path following simulation")
    parser.add_argument("--goal-threshold", type=float, default=0.2)
    parser.add_argument("--goal-x", type=float, default=1)
    parser.add_argument("--goal-y", type=float, default=1)
    parser.add_argument("--max-velocity-angular", type=float, default=1)
    parser.add_argument("--max-velocity-linear", type=float, default=0.3)
    parser.add_argument("--gain-orientation", type=float, default=1)
    parser.add_argument("--gain-position", type=float, default=0.2)
    parser.add_argument("--track-width", type=float, default=0.17)
    parser.add_argument("--time-step", type=float, default=0.1)
    parser.add_argument("--duration", type=float, default=800)
    args = parser.parse_args()

    poses = simulate(
        args.duration,
        args.time_step,
        args.track_width,
        args.goal_x,
        args.goal_y,
        args.goal_threshold,
        args.max_velocity_angular,
        args.max_velocity_linear,
        args.gain_orientation,
        args.gain_position,
    )

    xs = [p.x for p in poses]
    ys = [p.y for p in poses]

    print(poses[-1])

    plt.plot(xs, ys)
    plt.plot([args.goal_x], [args.goal_y], "o")
    plt.gca().set_aspect("equal", adjustable="box")
    plt.get_current_fig_manager().window.wm_geometry("+2600+400")
    plt.show()


if __name__ == "__main__":
    main()
