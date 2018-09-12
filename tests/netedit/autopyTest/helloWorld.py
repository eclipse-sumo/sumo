import autopy
import pyautogui

# simply open a dialog with "hello world"
def hello_world():
    autopy.alert.alert("Hello, world")

hello_world()