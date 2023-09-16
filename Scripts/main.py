# Python program to translate
# speech to text and text to speech

import speech_recognition as sr
import pyttsx3
import time

from enum import Enum

class Command(Enum):
    OPEN_DOOR  = 1
    CLOSE_DOOR = 2
    LIGHT_ON   = 3
    LIGHT_OFF  = 4

# Function to convert text to
# speech
def SpeakText(text):
    # Initialize the engine
    engine = pyttsx3.init()
    engine.say(text)
    engine.runAndWait()
    
def GetSpeechAsText(r):
    # use the microphone as source for input.
    with sr.Microphone() as source2:
        
        # wait for a second to let the recognizer
        # adjust the energy threshold based on
        # the surrounding noise level
        r.adjust_for_ambient_noise(source2, duration=1.5)
        
        #listens for the user's input
        audio2 = r.listen(source2)
        
        # Using google to recognize audio
        MyText = r.recognize_google(audio2)
        return MyText.lower()

# FSM states
# IDLE -> Listen for wake-up word
# WORK -> Awake, process commands
# EXIT -> Close the application
class State(Enum):
    IDLE  = 1
    WORK  = 2
    EXIT  = 3

# Functions associated with each command
# Each function should:
#   - accept the current state as a parameter
#   - return the next FSM state

# Wake-up word detected -> transition to WORK state 
def WakeUp(currentState):
    SpeakText("How can I help you?")
    return State.WORK

# Shut-down word detected -> transition to EXIT state
def ShutDown(currentState):
    print("Shutting down...")
    return State.EXIT

# Open the door and stay in same state
def OpenDoor(currentState):
    print("Opening the door...")
    return currentState

# Close the door and stay in same state
def CloseDoor(currentState):
    print("Closing the door...")
    return currentState

# Go-to-sleep word detected -> transition to IDLE state
def GoToSleep(currentState):
    SpeakText("Going back to sleep then")
    return State.IDLE

# Voice commands to execute
COMMANDS = {
    "wake up": WakeUp,
    "shut down": ShutDown,
    "open the door": OpenDoor,
    "close the door": CloseDoor,
    "go back to sleep": GoToSleep,
}

# FSM functions for each state
# Each function should:
#   - accept the voice command as text
#   - return the next state
def IdleStateFunc(text):
    next_state = State.IDLE

    if text in COMMANDS:
        next_state = COMMANDS[text](next_state)

    return next_state

def WorkStateFunc(text):
    next_state = State.WORK

    if text in COMMANDS:
        next_state = COMMANDS[text](next_state)

    return next_state

FSM = {
    State.IDLE.name: IdleStateFunc,
    State.WORK.name: WorkStateFunc,
}

def main():
    # Initialize state of FSM 
    state = State.IDLE

    # Initialize the recognizer
    r = sr.Recognizer()
    
    # Program loop
    while(1):
        # Exception handling
        try:
            text = GetSpeechAsText(r)

            state = FSM[state.name](text)

            if state == State.EXIT:
                break

        except sr.RequestError as e:
            print("Could not request results; {0}".format(e))

        except sr.UnknownValueError:
            print("Could not recognize speech...")

if __name__ == "__main__":
    main()