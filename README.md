# AI Image Generator (Gemini 3 Pro)

This project is a full-stack AI image generation application using the Plato API (Gemini 3 Pro Preview).

## Features
- **Image-to-Image Generation**: Upload an image and provide a prompt to generate/edit images.
- **Prompt Optimization**: Use Gemini 3 Pro to enhance your prompts.
- **Modern UI**: Glassmorphism design with particle background.

## Project Structure
- `front/`: Frontend static files (HTML/CSS/JS).
- `server/`: Backend Node.js/Express server.

## Local Development

1.  Navigate to the server directory:
    ```bash
    cd server
    ```
2.  Install dependencies:
    ```bash
    npm install
    ```
3.  Start the server:
    ```bash
    npm start
    ```
4.  Open `http://localhost:3011` in your browser.

## Deployment (Docker)

1.  Build the Docker image:
    ```bash
    docker build -t ai-image-gen .
    ```
2.  Run the container:
    ```bash
    docker run -d -p 3011:3011 --name ai-gen ai-image-gen
    ```

## Environment Variables
The API Key and URL are currently configured in `server/.env`. For production, pass them as environment variables to the Docker container.
