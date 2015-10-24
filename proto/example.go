// +build ignore

package main

import (
	"log"
	"os"

	oci "./go/"
	"github.com/golang/protobuf/jsonpb"
	"github.com/golang/protobuf/proto"
	google_protobuf "google/protobuf"
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

	user := &oci.LinuxUser{Uid: 1, Gid: 1, AdditionalGids: []int32{5, 6}}
	userBytes, err := proto.Marshal(user)
	if err != nil {
		log.Fatal(err)
	}
	s.Spec.Process.User = &google_protobuf.Any{
		TypeUrl: "oci.LinuxUser",
		Value:   userBytes,
	}

	marshaler := jsonpb.Marshaler{
		Indent:        "  ",
	}

	err = marshaler.Marshal(os.Stdout, s)
	if err != nil {
		log.Fatal(err)
	}
}
