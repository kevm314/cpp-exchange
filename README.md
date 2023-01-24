# cpp-exchange
A C++ based order matching system.


## Architecture overview

- This project architecture is inspired by the LMAX Disruptor architecture as explained [here](https://lmax-exchange.github.io/disruptor/user-guide/index.html).
- The `architecture` directory contains diagrams/sketches relating to the design of this project.

## Documentation

- To generate an up to date documentation (using doxygen) run the below command in the root project directory. This will generate an `index.html` file in the `docs/html/` directory of the project.
```
doxygen exchange-docs
```

## Running the exchange (TODO - fix up)

- Run the below executable from the `./build` folder found in the root project directory (the file path is that of the configuration file):
```
./exchange ../matchmaker/exchange_configuration.json ../events_receiver/data/three_users_playback.csv
```

## Dependencies

- Ensure to have docker installed using [this link](https://docs.docker.com/get-docker/).

## Development environment (using docker)

- To develop in a docker environment follow the steps below:
  - build the docker image using the commnd: 
```
docker build -t exchange .
```
- Run the docker container using the docker compose file:
```
docker compose up
```
- Attach to the running container using an IDE/code editor to dev within the container environment

## Testing

- To run testing for a given library or even the entire project, ensure to set the appropriate test variable to `ON` in the relevant `CMakelists.txt` file before rebuilding the executable.

## Resources

- [Google C++ style guide](https://google.github.io/styleguide/cppguide.html)

## TODO

- start implementation of matchmaker
  - implement main order book set up class
    - spawn each instrument as thread 
    - (matchmaker thread => debug spawner with csv streaming playback)
- tidy project folder structure:
  - messaging (ring buffer, producers, consumers)
  - receiver (producer that receives order events from users)
  - journaller (records incoming events)
  - matchmaker (order matching engine using incoming order events)
  - dispatcher (dispatches order outcomes back to users)
