FROM alpine:latest as BUILD_GCC

RUN apk update && apk add alpine-sdk gcc cmake ninja

WORKDIR /src
COPY ./server/ ./
WORKDIR /src/build
RUN cmake -S .. -DCMAKE_BUILD_TYPE=MinSizeRel -G Ninja && cmake --build .

FROM alpine:latest as BUILD_UI
RUN apk update && apk add nodejs npm
WORKDIR /src
COPY ./ui/ ./
RUN npm ci && npm run prod

FROM alpine:latest
RUN apk update && apk add --no-cache libstdc++
WORKDIR /srv/http
COPY --from=BUILD_GCC /src/build/jwx ./
COPY --from=BUILD_UI /src/build ./data
EXPOSE ${JWX_PORT:-4955}

ENTRYPOINT /srv/http/jwx --content-path /srv/http/data --port ${JWX_PORT:-4955}