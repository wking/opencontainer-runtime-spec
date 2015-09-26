// +build ignore

package main

import (
	"log"
	"os"

	oci "./go/"
	"github.com/golang/protobuf/jsonpb"
)

func main() {
	s := &oci.LinuxSpec{
		Spec: &oci.Spec{
			Platform: &oci.Platform{Os: "linux", Arch: "x86_64"},
			Process: &oci.Process{
				Cwd: "/",
				Env: []string{"TERM=linux"},
			},
		},
	}

	marshaler := jsonpb.Marshaler{
		EnumsAsString: true,
		Indent:        "  ",
	}

	err := marshaler.Marshal(os.Stdout, s)
	if err != nil {
		log.Fatal(err)
	}
}
