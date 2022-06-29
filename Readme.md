# JStefanelli.com code repository

This repository contains the latest stable iteration of [jstefanelli.com](http://jstefanelli.com)

## Sections

It's buit in 2 parts, divided in the 2 subdirectories.

### Server

This subdirectory contains a C++ static-files-only HTTP 1.1 server

### UI

This subdirectory contains the front-end code for the main page

## Building

Individual build instructions are provided in the respective `Readme.md` files in the correct subdirectories

## Deployment

The server is supposed to sit behind a proxy of some kind which handles HTTPS.

A Dockerfile is provided in the root directory. It will build both the server and frontend, and package it in a <10MB image.

### Docker image parameters:

 - **JWX_PORT**: Port for the server, default is *4955*
