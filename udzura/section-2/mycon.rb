#!/usr/bin/env hacorb
# -*- mode: ruby -*-
limit = ENV['MEMORY_LIMIT'] || "128m"
pid = Process.fork do
  Dir.mkdir "/sys/fs/cgroup/memory/kyusec-3-demo" rescue nil
  system "echo 0 > /sys/fs/cgroup/memory/kyusec-3-demo/memory.swappiness"
  system "echo #{limit} > /sys/fs/cgroup/memory/kyusec-3-demo/memory.limit_in_bytes"
  system "echo #{Process.pid} > /sys/fs/cgroup/memory/kyusec-3-demo/tasks"
  Namespace.setns(
    Namespace::CLONE_NEWNET,
    fd: File.open("/var/run/netns/kyusec-3-demo", 'r').fileno
  )
  Dir.chroot "/tmp/haconiwa/kyusec-3"
  Dir.chdir "/"
  Exec.execve ENV, "/usr/sbin/apache2ctl", "-D", "FOREGROUND"
end
p(Process.waitpid2 pid)
