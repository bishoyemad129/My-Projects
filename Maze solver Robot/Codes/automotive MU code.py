from microbit import *
from Freenove_Micro_Rover import *
from maze import *
from sequence import *

class MyMazeRobot:
    def __init__(self):
        self.rover = Micro_Rover()
        self.path = is_sequence(maze_2d, start, end) + "S"
        self.step_number = 0

        #Speed
        self.FAST = 90
        self.SLOW = 20

        #Time`
        self.FORWARD_TIME = 350
        self.TURN_TIME = 500

    def look_down(self):
        left = pin14.read_digital()
        center = pin15.read_digital()
        right = pin16.read_digital()
        return (left << 2) | (center << 1) | right

    def go_straight_through(self):
        while self.look_down() == 7:  # All eyes see the line
            self.rover.motor(self.FAST, self.FAST)

    def turn_right(self):
        display.show("<")
        self.rover.motor(self.FAST, self.FAST)
        sleep(self.FORWARD_TIME)
        self.rover.motor(self.FAST, -self.FAST)
        sleep(self.TURN_TIME)

    def turn_left(self):
        display.show(">")
        self.rover.motor(self.FAST, self.FAST)
        sleep(self.FORWARD_TIME)
        self.rover.motor(-self.FAST, self.FAST)
        sleep(self.TURN_TIME)

    def follow_the_line(self):
        eyes = self.look_down()

        if eyes in [2, 5]:
            self.rover.motor(self.FAST, self.FAST)

        elif eyes in [4, 6]:
            self.rover.motor(self.SLOW, self.FAST)

        elif eyes in [1, 3]:
            self.rover.motor(self.FAST, self.SLOW)

        elif eyes == 0:
            self.rover.motor(0, 0)
            sleep(1000)

        elif eyes == 7:
            self.pick_a_direction()

    def pick_a_direction(self):
        if self.step_number >= len(self.path):
            self.rover.motor(0, 0)
            return

        next_step = self.path[self.step_number]

        if next_step == "S":
            display.show("^")
            self.go_straight_through()
        elif next_step == "L":
            self.turn_left()
        elif next_step == "R":
            self.turn_right()

        self.step_number += 1

    def start(self):
        while True:
            display.show(Image.HAPPY)
            self.follow_the_line()

my_bot = MyMazeRobot()
my_bot.start()

