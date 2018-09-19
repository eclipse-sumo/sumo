import pyautogui

# simply open a dialog with "hello world"
def test_key():
    pyautogui.typewrite('Hello world!')  
    pyautogui.hotkey('ctrl', 'c',)

test_key() 