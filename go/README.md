## Prerequisite
-1. Put libqeep libraries(header files and shared library archive `*.a`) under directory `go/lib`\
-2. By default we set `QISPACE_API_ON` as `false`, if it is `true`, you need to ceate a `.env` file under directory `go`, with parameters `URL`(QiSpace Enterprise url) and `TOKEN`(device token)

## Run
```
go get github.com/joho/godotenv
go run demo_sequr.go
```