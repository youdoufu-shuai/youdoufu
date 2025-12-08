module.exports = {
  apps: [{
    name: "youdoufu-ai",
    script: "./index.js",
    watch: true,
    ignore_watch: ["node_modules", "generated_images"],
    env: {
      NODE_ENV: "development",
      PORT: 3011
    },
    env_production: {
      NODE_ENV: "production",
      PORT: 3011
    }
  }]
}
