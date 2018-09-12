import autopy
import time

# simply open a dialog with "hello world"
def test_key():
    autopy.key.tap("c", [autopy.key.Modifier.SHIFT])

test_key() 