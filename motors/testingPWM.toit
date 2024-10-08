import math show PI

import .motors

print-speed motors/Motors time-ms/int wheel-circumference-cm/float:
  time-delta := time-ms / 1000.0

  left-rot-per-s := motors.left-encoder.get-rotation-rate time-delta
  left-speed-cm-per-s := left-rot-per-s * wheel-circumference-cm

  right-rot-per-s := motors.right-encoder.get-rotation-rate time-delta
  right-speed-cm-per-s := right-rot-per-s * wheel-circumference-cm

  print "Left: $(%.2f left-speed-cm-per-s) cm/s, Right: $(%.2f right-speed-cm-per-s) cm/s"

main:
  wheel-circumference-cm := 7.0 * PI

  time-to-run := 3_000
  time-to-stop := 1_000
  time-between-updates := 500

  updates := time-to-run / time-between-updates

  half-speed := 0.5

  motors/Motors := Motors

  while true:

    print "Running forward..."
    motors.set-motors-speed-factor half-speed
    updates.repeat:
      print-speed motors time-between-updates wheel-circumference-cm
      sleep --ms=time-between-updates

    print "Stopping..."
    motors.stop
    sleep --ms=time-to-stop

    print "Running reverse..."
    motors.set-motors-speed-factor -half-speed
    updates.repeat:
      print-speed motors time-between-updates wheel-circumference-cm
      sleep --ms=time-between-updates

    print "Stopping..."
    motors.stop
    sleep --ms=time-to-stop

  motors.close
