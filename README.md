# OS2 Device Driver Assignment

## Build the driver

```bash
make
```

## Load the driver

```bash
sudo make load
```

## Check if the driver loaded correctly

```bash
lsmod | grep changeme
dmesg | tail
ls -l /dev/changeme-driver
```

## Read from the driver

```bash
sudo cat /dev/changeme-driver
```

## Write to the driver

```bash
echo "Hello from userspace" | sudo tee /dev/changeme-driver
```

## Reload the driver after modifications

```bash
sudo make unload && make && sudo make load
```

## Compile and run the test programs

```bash
gcc read_test.c -o read_test && sudo ./read_test
```

```bash
gcc read_write_test.c -o read_write_test && sudo ./read_write_test
```