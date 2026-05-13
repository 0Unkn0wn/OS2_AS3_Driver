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
lsmod | grep bastea
dmesg | tail
ls -l /dev/bastea-driver
```

## Read from the driver

```bash
cat /dev/bastea-driver
```

## Write to the driver

```bash
echo "Hello from userspace" | tee /dev/bastea-driver
```

## Reload the driver after modifications

```bash
sudo make unload && make && sudo make load
```

## Compile and run the test programs

```bash
gcc read_test.c -o read_test && ./read_test
```

```bash
gcc read_write_test.c -o read_write_test && ./read_write_test
```