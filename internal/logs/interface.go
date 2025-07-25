package logs

// This logger should output up to three lines at a time
// bottom-to-top and erase the last top one when new bottom
// one shows up
type Logger interface {
	Info(message string) 
	Warn(message string)
	Error(message string)
}
