import .motors

main:
  time-to-run := 3_000
  time-to-stop := 1_000
  time-between-updates := 500

  updates := time-to-run / time-between-updates

  half-speed := 0.5

  motors := Motors

  while true:

    print "Running forward..."
    motors.left-motor.set-speed -half-speed
    motors.right-motor.set-speed half-speed
    updates.repeat:
      print "Left: $(%.2f (motors.left-encoder.get-speed*100)), Right: $(%.2f (motors.right-encoder.get-speed*100))"
      sleep --ms=time-between-updates
  motors.close
