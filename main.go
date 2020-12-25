package main

import "fmt"

func main() {
	ch := make(chan *KeyEvent)
	registerChan(ch)
	startup()
	for {
		select {
		case ev := <-ch:
			if ev.State == KEY_STATE_RELEASED {
				fmt.Printf("KeyEvent %s(%d) state: released.\n", globalKeyMap[ev.Keycode], ev.Keycode)
			} else if ev.State == KEY_STATE_PRESSED {
				fmt.Printf("KeyEvent %s(%d) state: pressed.\n", globalKeyMap[ev.Keycode], ev.Keycode)
			} else {
				fmt.Printf("KeyEvent %s(%d) state: unknown.\n", globalKeyMap[ev.Keycode], ev.Keycode)
			}
		}
	}
}
