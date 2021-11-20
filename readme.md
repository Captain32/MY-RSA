# My RSA

## keygen

生成公私钥对，option如下：

```
Options:
    -b:specifies the minimum bits needed for the public modulus n (default 256)
    -i:specifies the number of Miller-Rabin iterations for testing primes (default:50)
    -n:specifies the public key file (default:rsa.pub)
    -d:specifies the private key file (default:rsa.priv)
    -s:specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL))
    -v:enables verbose output
    -h:displays program synopsis and usage.
```

## encrypt

加密文件，option如下：

```
Options:
    -i:specifies the input file to encrypt (default:stdin)
    -o:specifies the output file to encrypt (default:stdout)
    -n:specifies the file containing the public key (default:rsa.pub)
    -v:enables verbose output
    -h:displays program synopsis and usage.
```

## decrypt

解密文件，option如下：

```
Options:
    -i:specifies the input file to encrypt (default:stdin)
    -o:specifies the output file to encrypt (default:stdout)
    -n:specifies the file containing the private key (default:rsa.priv)
    -v:enables verbose output
    -h:displays program synopsis and usage.
```