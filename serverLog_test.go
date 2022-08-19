package main

import (
	"io"
	"reflect"
	"testing"
)

type (
	mockReadCloser struct {
		expectedData []byte
		expectedErr  error
	}
)

// allow subtests to fill out expectedData and expectedErr field
func (mrc *mockReadCloser) Read(p []byte) (n int, err error) {
	copy(p, mrc.expectedData)
	return 0, mrc.expectedErr
}

// do nothing on close
func (mrc *mockReadCloser) Close() error { return nil }

func TestGetLog(t *testing.T) {
	subtests := []struct {
		name         string
		rc           io.ReadCloser
		numBytes     int
		expectedData []byte
		expectedErr  error
	}{
		{
			name: "happy path",
			rc: &mockReadCloser{
				expectedData: []byte(`hello\n`),
				expectedErr:  nil,
			},
			numBytes:     5,
			expectedData: []byte(`hello\n`),
			expectedErr:  nil,
		},
	}
	for _, subtest := range subtests {
		t.Run(subtest.name, func(t *testing.T) {
			data := NewServerLog(subtest.rc).GetLog()
			if !reflect.DeepEqual(data, subtest.expectedData) {
				t.Errorf("GetLog method produced wrong result. expected: %s, got: %s", subtest.expectedData, data)
			}
		})
	}
}

func TestTrimNewLine(t *testing.T) {
	testStr := "test\n"
	if got, want := trimNewLine([]byte(testStr)), "test"; got != want {
		t.Errorf("trimNewLine method produced wrong result. expected: %s, got: %s", want, got)
	}
}
