package logs

const TOTAL_OUT_LINES = 4

type OutLogger struct {
	buffer [TOTAL_OUT_LINES]string
}

func GetNewOutLogger() Logger {
	return &OutLogger{}
}

func (l *OutLogger) Info(message string) {}

func (l *OutLogger) Warn(message string) {}

func (l *OutLogger) Error(message string) {}
