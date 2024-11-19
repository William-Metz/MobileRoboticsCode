#!/opt/homebrew/Caskroom/miniforge/base/envs/visualize/bin/python
# You probably want to change the above line to: #!/usr/bin/env python

from __future__ import annotations

from dataclasses import dataclass
from math import atan2, cos, pi, sin

import matplotlib
import matplotlib.pyplot as plt

matplotlib.use('TkAgg')

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
        K_orientation: float,
        K_position: float,
    ):
        self.goal_x = goal_x
        self.goal_y = goal_y

        self.max_velocity_angular = max_velocity_angular
        self.max_velocity_linear = max_velocity_linear

        self.track_width = track_width

        self.K_orientation = K_orientation
        self.K_position = K_position

    def update(self, pose: Pose, threshold: float) -> tuple[float, float]:
        dx = self.goal_x - pose.x
        dy = self.goal_y - pose.y

        d_error = ((dx * dx) + (dy * dy)) ** 0.5

        if d_error < threshold:
            return 0, 0

        v = min(self.K_position * d_error, self.max_velocity_linear)

        theta_to_goal = atan2(dy, dx)
        theta_error = theta_to_goal - pose.theta
        theta_dot = min(self.K_orientation * theta_error, self.max_velocity_angular)
        theta_dot = max(theta_dot, -self.max_velocity_angular)

        left_velocity = v - theta_dot * self.track_width / 2
        right_velocity = v + theta_dot * self.track_width / 2

        return left_velocity, right_velocity

def simulate(duration: float,
             time_step: float,
             track_width: float,
             goal_x: float,
             goal_y: float,
             goal_threshold: float,
             max_velocity_angular: float,
             max_velocity_linear: float,
             K_orientation: float,
             K_position: float) -> list[Pose]:

    time = 0

    forward_kinematics = ForwardKinematics(track_width)
    position_control = GoalPositionControl( goal_x, goal_y, max_velocity_angular,
                                           max_velocity_linear, track_width,
        K_orientation, K_position
    )

    velocity_left = 0
    velocity_right = 0

    poses = [forward_kinematics.pose.copy()]

    # Numerical simulation loop
    while time < duration:
        pose = forward_kinematics.update(velocity_left, velocity_right, time_step)
        velocity_left, velocity_right = position_control.update(pose, goal_threshold)

        poses.append(pose.copy())

        time += time_step

    return poses

def main():
    # TODO: define an argument parser
    # TODO: program arguments

    GOAL_X = -1
    GOAL_Y = 1
    GOAL_THRESHOLD = 0.2

    MAX_VELOCITY_ANGULAR = 1
    MAX_VELOCITY_LINEAR = 0.3
    K_ORIENTATION = 1
    K_POSITION = 0.2

    TRACK_WIDTH = 0.17

    TIME_STEP = 0.1
    DURATION = 20

    poses = simulate(DURATION, TIME_STEP, TRACK_WIDTH, GOAL_X, GOAL_Y, GOAL_THRESHOLD,
             MAX_VELOCITY_ANGULAR, MAX_VELOCITY_LINEAR, K_ORIENTATION, K_POSITION)

    xs = [p.x for p in poses]
    ys = [p.y for p in poses]

    print(poses[-1])

    plt.plot(xs, ys)
    plt.plot([GOAL_X], [GOAL_Y], 'o')
    plt.gca().set_aspect('equal', adjustable='box')
    plt.get_current_fig_manager().window.wm_geometry('+2600+400')
    plt.show()

if __name__ == '__main__':
    main()
