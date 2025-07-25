package containers

const MAX_SIZE = 4

type Node struct {
	Content string
	Next    *Node
}

type LinkedList struct {
	size int
	tail *Node
	head *Node
}

func NewLinkedList() *LinkedList {
	return &LinkedList{
		size: 0,
		tail: nil,
		head: nil,
	}
}

func (ll *LinkedList) Head() *Node {
	return ll.head
}

func (ll *LinkedList) Tail() *Node {
	return ll.tail
}

func (ll *LinkedList) PushAndFetch(buffer string) string {
	ll.push(buffer)
	return ll.latest()
}

func (ll *LinkedList) Clear() {
	current := ll.head
	for current != nil {
		next := current.Next
		current.Next = nil 
		current = next
	}

	ll.head = nil
	ll.tail = nil
	ll.size = 0
}


func (ll *LinkedList) Empty() bool {
	return ll.size == 0
}

func (ll *LinkedList) push(buffer string) {
	newNode := &Node{
		Content: buffer,
		Next:    nil,
	}

	if ll.Empty() {
		ll.head = newNode
		ll.tail = newNode
		ll.size = 1
		return
	}

	// If at max capacity, remove first node
	if ll.size == MAX_SIZE {
		ll.head = ll.head.Next
		ll.size--
	}

	// Insert new node at back
	ll.tail.Next = newNode
	ll.tail = newNode
	ll.size++
}

func (ll *LinkedList) latest() string {
	return ll.tail.Content 
}

