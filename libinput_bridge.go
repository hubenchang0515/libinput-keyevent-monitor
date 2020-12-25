package main

// #include "libinput_bridge.h"
// #cgo pkg-config: libinput glib-2.0
// #cgo LDFLAGS: -ludev -lm
import "C"

const (
	KEY_STATE_RELEASED = 0
	KEY_STATE_PRESSED  = 1
)

type KeyEvent struct {
	Keycode uint32
	State   uint32 // KEY_STATE_RELEASED,KEY_STATE_PRESSED
}

var eventChanList []chan *KeyEvent

func startup() {
	go func() {
		C.loop_startup()
	}()
}

func stop() {
	C.loop_stop()
}

func registerChan(ch chan *KeyEvent) {
	eventChanList = append(eventChanList, ch)
}

//export pushKeyEvent
func pushKeyEvent(keycode uint32, state uint32) {
	event := &KeyEvent{
		Keycode: keycode,
		State:   state,
	}

	for _, ch := range eventChanList {
		select {
		case ch <- event:
		default:
		}
	}
}
