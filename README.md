# cpp-exchange
A C++ based order matching system.


## Architecture overview

- This project architecture is inspired by the LMAX Disruptor architecture as explained [here](https://lmax-exchange.github.io/disruptor/user-guide/index.html).

## Dependencies

- Ensure to have docker installed using [this link](https://docs.docker.com/get-docker/).

## Development environment (using docker)

- To develop in a docker environment follow the steps below:
  - build the docker image using the commnd: 
```
$ docker build -t exchange .
```
- Run the docker container using the docker compose file:
```
$ docker compose up
```
- Attach to the running container using an IDE/code editor to dev within the container environment

## Resources

- [Google C++ style guide](https://google.github.io/styleguide/cppguide.html)

## TODO

- add gtest
- add doxygen
- add logging library
- tidy project folder structure:
  - messaging (ring buffer, producers, consumers)
  - receiver (producer that receives order events from users)
  - journaller (records incoming events)
  - matchmaker (order matching engine using incoming order events)
  - dispatcher (dispatches order outcomes back to users)
