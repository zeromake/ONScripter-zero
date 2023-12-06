package main

import (
	"log"
	"os"

	"github.com/zeromake/pkg/binaryutil"
)

func main1() {
	input, err := os.Open(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	r := binaryutil.NewDecoder(input)
	r.Skip(4)
	size := r.ReadU32LE()
	log.Println("size:", size)
	name := make([]uint8, 32)
	r.ReadBE(name)
	offset := 32
	for name[offset-1] == 0 {
		offset--
	}
	log.Println("name:", string(name[:offset]))
	n := r.ReadU32LE()
	r.Skip(4)
	for i := uint32(0); i < n; i++ {
		x := r.Read32LE()
		y := r.Read32LE()
		w := r.Read32LE()
		h := r.Read32LE()
		r.Skip(3 * 4)
		log.Printf("item: [%d, %d, %d, %d]\n", x, y, w, h)
	}
}
