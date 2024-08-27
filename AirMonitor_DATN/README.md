## Build Instruction

### For developer (Recommended)

- From the project root folder, `cd front` and `npm install` to install dependencies in front. Similarly, `cd server` and `npm install` to install dependencies in server.

- After that, run `npm run start` in the project root folder. The frontend will be on `http://localhost:1444` and server will be on `http://localhost:1443`

### For user

- Firstly, you need to install Docker
    - For Windows, you can install Docker Desktop: [here](https://docs.docker.com/desktop/install/windows-install/)
    - If you are using Linux, you should install Docker Engine, for working consistency in our workflow: [here](https://docs.docker.com/engine/install/)

- After installed Docker, you can run `docker compose up` in the project root folder, the app will be on `http://YOUR_LOCAL_IP:1080`. (You can find your local IP address using the `ipconfig` command on Windows, or the `ifconfig` on Linux)