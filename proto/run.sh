#!/bin/bash

set -e

username="protobuild"
groupname="${username}"
p_dir="/proto/"
o_dir="/output/"

uid=$(stat -c "%u" "${p_dir}")
gid=$(stat -c "%g" "${p_dir}")
groupadd -g "${gid}" "${groupname}"
adduser -u "${uid}" -g "${gid}" "${username}"
chown -R "${username}:${groupname}" "${o_dir}"
cd "${p_dir}"
su "${username}" -c "make go py cpp DESTDIR='${o_dir}'"

