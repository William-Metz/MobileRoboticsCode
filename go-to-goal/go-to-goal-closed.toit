import display show Display
import communication show Communicator WsCommunication
import ..led
import ..motors
import ..pinout
import ..utilities
class LedBlinker implements Communicator:
  state := Communicator.DISABLED

  led/Led := Led
  display/Display := Display SDA-PIN SCL-PIN 
  motors/Motors := Motors
    
  constructor:
    led.off
    motors.stop

  on-start address port:
    display.add-text "$address"
    display.add-text --y=32 "$port"
    display.draw

  on-open: enable
  on-close: disable
  on-message message:
    print "Received message: $message"
    enable

  is-enabled:
    return state == Communicator.ENABLED

  enable:
    if state == Communicator.ENABLED: return
    print "Enabling"
    state = Communicator.ENABLED
    led.on

  disable:
    if state == Communicator.DISABLED: return
    print "Disabling"
    state = Communicator.DISABLED
    led.off
    motors.stop
class ProportionalControl:

  kp/float
  speed-factor/float := 0.0
  max-speed-step/float
  max-speed/float

  constructor .kp/float .max-speed-step/float .max-speed/float:


  // u = Kp * e
  compute-speed-factor desired-speed/float measured-speed/float:
    desired-speed = constrain desired-speed -max-speed max-speed
    error := desired-speed - measured-speed
    u := constrain kp*error -max-speed-step max-speed-step

    speed-factor = constrain (speed-factor + u) -1.0 1.0
    return speed-factor
class MotorControl:

  static WHEEL-CIRCUMFERENCE ::= .7

  static Left-KP ::= 0.1
  static Right-KP::= 0.1
  static max-speed-step ::= 0.1
  static max-speed ::= 0.9

  motors/Motors

  left-controller/ProportionalControl := ProportionalControl Left-KP max-speed-step max-speed
  right-controller/ProportionalControl := ProportionalControl Right-KP max-speed-step max-speed
  left-time/int 
  right-time/int 


  constructor .motors:
    left-time = Time.monotonic-us
    right-time = Time.monotonic-us



  update-forward-speed desired-speed/float:
    // Update Left motor
    now := Time.monotonic-us
    time-delta :=(now - left-time).to-float/ 1_000_000.0
    left-rot-per-s := motors.left-encoder.get-rotation-rate time-delta
    left-speed := left-rot-per-s*WHEEL-CIRCUMFERENCE
    print left-speed
    left-speed-factor := left-controller.compute-speed-factor desired-speed left-speed
    motors.left_motor.set-speed-factor left-speed-factor 
    //Update Right Motor
    now = Time.monotonic-us
    time-delta =(now - right-time).to-float/ 1_000_000.0
    right-rot-per-s := motors.right-encoder.get-rotation-rate time-delta
    right-speed := right-rot-per-s*WHEEL-CIRCUMFERENCE
    right-speed-factor := right-controller.compute-speed-factor desired-speed right-speed
    motors.right_motor.set-speed-factor right-speed-factor 


main:
    led-blinker := LedBlinker
    comm := WsCommunication led-blinker --heartbeat-ms=1000
    

    while not led-blinker.is-enabled:
      sleep --ms=1000


    motor-control := MotorControl led-blinker.motors


    motors-speed := 0.90
    duration-ms := 50_000

    control-update_ms := 100
    time:= 0


    while time < duration-ms and led-blinker.is-enabled:

      motor-control.update-forward-speed motors-speed
      sleep --ms= control-update_ms
      time += control-update_ms
    
    led-blinker.motors.stop
    print "Done"

