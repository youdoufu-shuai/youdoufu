FROM node:18-alpine

# Set working directory
WORKDIR /app

# Install dependencies first (caching)
COPY server/package*.json ./server/
WORKDIR /app/server
RUN npm install --production

# Copy source code
WORKDIR /app
COPY . .

# Start server
WORKDIR /app/server
EXPOSE 3011
CMD ["npm", "start"]
