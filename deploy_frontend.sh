echo "Deploying Frontend..."
cd webpage
npm run build
aws s3 sync build/ s3://roomlights-frontend