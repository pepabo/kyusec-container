## Dockerをインストール

```bash
sudo apt-get install \
    apt-transport-https \
    ca-certificates \
    curl \
    software-properties-common

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"

sudo apt-get install docker-ce

sudo gpasswd -a vagrant docker

# relogin...
docker info
```

## Docker上でapacheを動かそう

```bash
docker build -t kyusec-1 udzura/section-2
docker run -p 8080:80 -d kyusec-1
curl localhost:8080 -s | grep '<title>'

# Check!
ps auxf

sudo apt-get install apache2
systemctl start apache2
# Re check!!
ps auxf
```

## Linux Namespace

```bash
docker ps
CID=88b7e...
docker exec -ti $CID bash
# ip a

# Compare:
ip a

# Also compare
docker exec -ti $CID hostname
hostname

docker exec -ti $CID ps auxf
ps auxf
```

### nsを表現するfileの確認

```bash
ps auxf | grep -A 6 docker[d]
PID=...

sudo ls -l /proc/$PID/ns
sudo ls -l /proc/self/ns
# Doing diff by your phisical eyes!

sudo nsenter --net -t $PID
# ip a ...
```

## cgroup

```bash
CID=$(docker inspect -f '{{.ID}}' $CID)
sudo cat /sys/fs/cgroup/memory/docker/$CID/memory.usage_in_bytes
sudo cat /sys/fs/cgroup/memory/docker/$CID/memory.limit_in_bytes

CID2=$(docker run --memory=4m -d kyusec-1); echo $CID2
sudo cat /sys/fs/cgroup/memory/docker/$CID2/memory.usage_in_bytes
sudo cat /sys/fs/cgroup/memory/docker/$CID2/memory.limit_in_bytes
```

```bash
docker exec -ti $CID bash
# apt install ruby...
docker exec -ti $CID2 bash

echo '128m' | sudo tee /sys/fs/cgroup/memory/docker/$CID2/memory.limit_in_bytes
docker exec -ti $CID2 bash   
root@ebb02470253b:/# apt install ruby ...
```

### cgroup 自作編

```bash
sudo mkdir /sys/fs/cgroup/memory/kyusec-2
sudo ls -l  /sys/fs/cgroup/memory/kyusec-2
echo 0 | sudo tee /sys/fs/cgroup/memory/kyusec-2/memory.swappiness
echo 4m | sudo tee /sys/fs/cgroup/memory/kyusec-2/memory.limit_in_bytes

echo $$ | sudo tee /sys/fs/cgroup/memory/kyusec-2/tasks
ruby -e \
  'GC.disable;ha=Hash.new;loop{1000.times{ha[rand(2**20).to_s.to_sym]=rand(2**20).to_s.to_sym}}'

# 割り当てを変える
echo 1g | sudo tee /sys/fs/cgroup/memory/kyusec-2/memory.limit_in_bytes
ruby -e \
  'GC.disable;ha=Hash.new;loop{1000.times{ha[rand(2**20).to_s.to_sym]=rand(2**20).to_s.to_sym}}'
# 即死しないことが確認できる
```

### pids controller の実験

```bash
docker run --pids-limit=128 -ti ubuntu:xenial bash
# paste the forkbomb code!!!
# Then survive

CID3=$(docker run --pids-limit=128 -d ubuntu:xenial sleep 128)
sudo cat /sys/fs/cgroup/pids/docker/$CID3/pids.max
# => ?
```
