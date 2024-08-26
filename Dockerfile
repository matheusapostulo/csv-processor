FROM --platform=linux/amd64 alpine:3.20.1

WORKDIR /app

COPY . .

RUN ./build.sh

CMD ["/app/test_libcsv"]
