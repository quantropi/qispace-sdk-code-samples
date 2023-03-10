## Prerequisite
-1. Put libqeep libraries(header files and shared library archive `*.a`) under directory `go/lib`
-2. Ceate a `.env` file under directory `go`, with parameters `URL`(enterprise url) and `TOKEN`(device token)
    ```
    URL="https://xxx"
    TOKEN="xxxx"
    ```

## Run
```
go get github.com/joho/godotenv
go run demo_sequr.go
```