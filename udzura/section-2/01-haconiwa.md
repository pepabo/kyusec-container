## Haconiwa 演習

### install

```bash
curl -s https://packagecloud.io/install/repositories/udzura/haconiwa/script.deb.sh | sudo bash
sudo apt-get install haconiwa=0.10.0~alpha2-1
haconiwa version
```

### setup image

```bash
sudo mkdir -p /tmp/haconiwa/kyusec-3
docker export 88b7e296fe92 | sudo tar -xv -f - -C /tmp/haconiwa/kyusec-3
sudo chroot /tmp/haconiwa/kyusec-3 ls -l
```

### setup haconiwa environment

```bash
sudo haconiwa init --bridge
haconiwa init kyusec-3.haco
# vim it!
```

```bash
sudo haconiwa run kyusec-3.haco
curl -s 10.0.0.10 | grep '<title>'
```

### run your container by hacorb

```
## Prepare networking
sudo ip netns add kyusec-3-demo
sudo ip link add kyusec-3-host type veth peer name kyusec-3-guest
sudo brctl addif haconiwa0 kyusec-3-host
sudo ip link set kyusec-3-guest netns kyusec-3-demo up
sudo ip link set kyusec-3-host up
sudo ip netns exec kyusec-3-demo ip link set lo up
sudo ip netns exec kyusec-3-demo ip addr add 10.0.0.20/24 dev kyusec-3-guest

ping 10.0.0.20

## Then:
sudo hacorb mycon.rb
curl 10.0.0.20 -s | grep title

sudo env MEMORY_LIMIT='1m' hacorb mycon.rb
```
