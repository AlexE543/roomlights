echo "Deploying Backend..."
cd api
aws ecr get-login-password --region us-east-2 | docker login --username AWS --password-stdin 497979473935.dkr.ecr.us-east-2.amazonaws.com
docker build -t roomlights-backend .
docker tag roomlights-backend:latest 497979473935.dkr.ecr.us-east-2.amazonaws.com/roomlights-backend:latest
docker push 497979473935.dkr.ecr.us-east-2.amazonaws.com/roomlights-backend:latest
cd aws_deploy
eb deploy