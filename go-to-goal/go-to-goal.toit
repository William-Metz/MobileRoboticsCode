import display show Display
import communication show Communicator WsCommunication
import ..led
import ..motors
import ..pinout
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

main:
    led-blinker := LedBlinker
    comm := WsCommunication led-blinker --heartbeat-ms=1000
    ip-address := "$comm.ip-address :$comm.port"

    while true:
      if led-blinker.is-enabled:
        led-blinker.motors.set-motors-speed-factor .5

        sleep --ms=5_000
        led-blinker.motors.stop

        break
      else:
        sleep --ms=1_000



    
