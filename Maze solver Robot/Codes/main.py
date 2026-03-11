from microbit import *
from Freenove_Micro_Rover import Micro_Rover

Rover = Micro_Rover()
display.show(Image.HAPPY)

# Track turn sequence
turn_count = 0
TURN_TIME = 950  # Time for turns in milliseconds

# turn sequence
turn_sequence = ["S", "R", "S", "S", "L", "S", "R", "S"]

while True:
    # Read sensors and combine into a 3-bit value
    sensor_value = (pin14.read_digital() << 2) | \
                   (pin15.read_digital() << 1) | \
                   (pin16.read_digital())

    # 1) NO LINE => STOP
    if sensor_value == 0:
        Rover.motor(0, 0)
        Rover.all_led_show(255, 0, 0, 0)  # Red to indicate no line
        continue

    # 2) INTERSECTION DETECTED => Execute turn sequence
    if sensor_value == 7:  # All sensors see black
        if turn_count < len(turn_sequence):  # Check if there are more turns in the sequence
            action = turn_sequence[turn_count]
            print("Intersection #", turn_count + 1, "Action:", action)

            if action == "L":  # Turn left
                Rover.all_led_show(255, 255, 0, 0)  # Yellow for left turn
                Rover.motor(0, 60)  # Turn left
                sleep(TURN_TIME)
                Rover.motor(0, 0)
                sleep(200)
                # Move forward to reacquire line
                Rover.motor(80, 80)
                sleep(400)
                Rover.motor(0, 0)

            elif action == "R":  # Turn right
                Rover.all_led_show(0, 0, 255, 0)  # Blue for right turn
                Rover.motor(60, 0)  # Turn right
                sleep(TURN_TIME)
                Rover.motor(0, 0)
                sleep(200)
                # Move forward to reacquire line
                Rover.motor(80, 80)
                sleep(400)
                Rover.motor(0, 0)

            elif action == "S":  # Go straight
                Rover.all_led_show(0, 255, 0, 0)  # Green for going straight
                Rover.motor(80, 80)
                sleep(500)
                Rover.motor(0, 0)
                sleep(100)

            turn_count += 1  # Move to the next action in the sequence

        else:
            # No more turns in the sequence
            Rover.motor(0, 0)
            Rover.all_led_show(255, 255, 255, 255)  # White
      