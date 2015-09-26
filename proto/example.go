// +build ignore

package main

import (
	"encoding/json"
	"log"

	oci "./go/"
	"github.com/golang/protobuf/proto"
)

func main() {
	s := oci.LinuxSpec{
		Spec: &oci.Spec{
			Version: proto.String("0.1.0"),
			Platform: &oci.Platform{Os: proto.String("linux"), Arch: proto.String("x86_64")},
			Process: &oci.Process{
				Terminal: proto.Bool(true),
				User: &oci.User{Type: oci.PlatformType_LINUX.Enum()},
				Args: []string{"sh"},
				Cwd: proto.String("/root"),
				Env: []string{"TERM=linux"},
			},
		},
	}

	err := proto.SetExtension(s.Spec.Process.User, oci.E_Uid, proto.Int32(1))
	if err != nil {
		log.Fatal(err)
	}

	err = proto.SetExtension(s.Spec.Process.User, oci.E_Gid, proto.Int32(1))
	if err != nil {
		log.Fatal(err)
	}

	err = proto.SetExtension(s.Spec.Process.User, oci.E_AdditionalGids, []int32{5, 6})
	if err != nil {
		log.Fatal(err)
	}

	buf, err := json.MarshalIndent(s, "", "  ")
	if err != nil {
		log.Fatal(err)
	}
	println(string(buf))
}
