package containers

import (
	"testing"
)

func TestPushAndFetch(t *testing.T) {
	ll := NewLinkedList()
	got := ll.PushAndFetch("first")
	if got != "first" {
		t.Errorf("Expected 'first', got '%s'", got)
	}

	ll.PushAndFetch("second")
	ll.PushAndFetch("third")
	got = ll.PushAndFetch("fourth")
	if got != "fourth" {
		t.Errorf("Expected 'fourth', got '%s'", got)
	}
}

func TestPushBeyondMaxSize(t *testing.T) {
	ll := NewLinkedList()

	ll.PushAndFetch("1")
	ll.PushAndFetch("2")
	ll.PushAndFetch("3")
	ll.PushAndFetch("4")
	ll.PushAndFetch("5") // Exceeds MAX_SIZE (4)

	if ll.size != MAX_SIZE {
		t.Errorf("Expected size %d, got %d", MAX_SIZE, ll.size)
	}

	// Head should now be "2", since "1" was dropped
	if ll.Head().Content != "2" {
		t.Errorf("Expected head to be '2', got '%s'", ll.Head().Content)
	}

	// Tail should be "5"
	if ll.Tail().Content != "5" {
		t.Errorf("Expected tail to be '5', got '%s'", ll.Tail().Content)
	}
}

func TestClear(t *testing.T) {
	ll := NewLinkedList()

	ll.PushAndFetch("a")
	ll.PushAndFetch("b")
	ll.Clear()

	if !ll.Empty() {
		t.Errorf("Expected list to be empty after Clear()")
	}

	if ll.Head() != nil || ll.Tail() != nil {
		t.Errorf("Expected both head and tail to be nil after Clear()")
	}
}

func TestEmpty(t *testing.T) {
	ll := NewLinkedList()
	if !ll.Empty() {
		t.Errorf("Expected new list to be empty")
	}
	ll.PushAndFetch("x")
	if ll.Empty() {
		t.Errorf("Expected list to be non-empty after Push")
	}
}

