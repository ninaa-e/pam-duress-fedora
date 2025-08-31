
```
cd ~
mkdir .duress
cd .duress
```
```
echo -n "1312" | sha256sum | awk '{print $1}' > password_hash
chmod 600 password_hash
```

```
sudo nano duress_action.sh
sudo chmod +x duress_action.sh
sudo chcon -t bin_t duress_action.sh
```