## Prerequisite
1. Put libqeep libraries(header files and shared library archive `*.a`) under the directory `go/lib`\
2. By default we set `QISPACE_API_ON` as `false`, if it is `true`, you need to create a `.env` file under directory `go`, with parameters `URL`(QiSpace Enterprise URL) and `TOKEN`(device token)

## Run
```
go get github.com/joho/godotenv
go run demo_sequr.go
```