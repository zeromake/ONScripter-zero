package main

import (
	"encoding/base64"
	"log"
	"os"
)

func main() {
	var cert = os.Args[1]
	var out = "./cert.tar.xz"
	log.Println("cert: ", cert[0:4])
	bb, err := base64.StdEncoding.DecodeString(cert)
	if err != nil {
		log.Fatal(err)
	}
	f, err := os.OpenFile(out, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatal(err)
	}
	_, err = f.Write(bb)
	if err != nil {
		log.Fatal(err)
	}
	err = f.Close()
	if err != nil {
		log.Fatal(err)
	}
	log.Println("done")
}
