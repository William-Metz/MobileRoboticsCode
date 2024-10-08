import ..display
import ..communication
import ..led
import ..motors
class LedBlinker implements Communicator:
  state := Communicator.DISABLED

  led/Led := Led
  display/Display := Display
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
        led-blinker.motors.set-motors-speed-factor .75

        sleep --ms=7_500
        led-blinker.motors.stop

        break
      else:
        sleep --ms=1_000



    
