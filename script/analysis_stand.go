package main

import (
	"fmt"
	"log"
	"os"
	"strings"

	"github.com/zeromake/pkg/binaryutil"
)

func to_string(buf []uint8) string {
	offset := 32
	for offset > 0 && buf[offset-1] == 0 {
		offset--
	}
	return string(buf[:offset])
}

func main() {
	input, err := os.Open(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	defer input.Close()
	r := binaryutil.NewDecoder(input)
	output, err := os.OpenFile(
		os.Args[2],
		os.O_CREATE|os.O_WRONLY|os.O_TRUNC,
		0644,
	)
	if err != nil {
		log.Fatal(err)
	}
	defer output.Close()
	output.WriteString("fill,img1,ani,img2,x,y\n")

	for r.Error() == nil {
		r.Skip(4)
		if r.Error() != nil {
			break
		}
		imgBuf := make([]uint8, 32)
		r.ReadLE(imgBuf)
		output.WriteString(to_string(imgBuf))
		output.Write([]byte{','})
		r.ReadLE(imgBuf)
		output.WriteString(to_string(imgBuf))
		output.Write([]byte{','})
		r.ReadLE(imgBuf)
		output.WriteString(to_string(imgBuf))
		r.ReadLE(imgBuf)
		if imgBuf[0] == 0 {
			r.Skip(8)
			output.Write([]byte{'\n'})
			continue
		}
		img := to_string(imgBuf)
		output.Write([]byte{','})
		output.WriteString(img)
		output.Write([]byte{','})
		x := r.Read32LE()
		y := r.Read32LE()
		if strings.HasPrefix(img, "UMI_") {
			if strings.HasPrefix(img, "UMI_A") {
				x -= 185
				y -= 57
			} else if strings.HasPrefix(img, "UMI_0101") || strings.HasPrefix(img, "UMI_0201") {
				x -= 237
				y -= 57
				// x -= 237
				// y -= 57
			} else {
				x -= 280
				y -= 66
			}
		}
		output.WriteString(fmt.Sprintf("%d,%d", x, y))
		output.Write([]byte{'\n'})
	}
}
