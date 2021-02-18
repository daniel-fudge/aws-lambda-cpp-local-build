# aws-lambda-cpp-local-build
This repo build a C++ Lambda function on a local workstation and then deploys it with the CLI.   
A YouTube walk through of this repo can also be found [here](https://youtu.be/LaDrQqrrmrI).

 ## Install Some Dependencies
 ```bash
sudo apt install cmake
sudo apt install make
sudo apt install zip
sudo apt install libcurl4-openssl-dev
sudo apt install libssl-dev
```

## Build the AWS C++ SDK
These steps install the C++ SDK into the `~/install` directory and clones all the directories into `~`. This can obviously be altered if desired.    
We are also only installing the `core` package. Other packages may be required for more involved Lambda functions. 
```bash
mkdir ~/install
cd ~
git clone https://github.com/aws/aws-sdk-cpp.git
cd aws-sdk-cpp
mkdir build
cd build
cmake .. -DBUILD_ONLY="core" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_UNITY_BUILD=ON \
  -DCUSTOM_MEMORY_MANAGEMENT=OFF \
  -DCMAKE_INSTALL_PREFIX=~/install \
  -DENABLE_UNITY_BUILD=ON
make
make install
```

## Build the AWS C++ Lambda Runtime
```bash
cd ~
git clone https://github.com/awslabs/aws-lambda-cpp-runtime.git
cd aws-lambda-cpp-runtime
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_INSTALL_PREFIX=~/install
make
make install
```

## Build the Actual C++ Lambda Function
```bash
git clone git@github.com:daniel-fudge/aws-lambda-cpp-local-build.git
cd ~/aws-lambda-cpp-local-build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/install
make
make aws-lambda-package-demo
```

## Make IAM Role for the Lambda Function
We need to create and IAM role that the can be attached to the lambda function when it is deployed. Note the rights in this role may need to be expanded for more complex Lambda functions.  
First create a JSON file that defines the required permissions as shown below.
```JSON
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Principal": {"Service": ["lambda.amazonaws.com"]},
      "Action": "sts:AssumeRole"
    }
  ]
}
```
Then create the IAM role in the CLI as shown below.
```bash
aws iam create-role --role-name lambda-demo --assume-role-policy-document file://trust-policy.json
```
The output of the above command will include the ARN of the new role. You must copy this ARN. It will be required when you deploy the Lambda function. It will most like have the form `arn:aws:iam::<your AWS account number>:role/lambda-demo`.   

Next attached the `AWSLambdaBasicExecutionRole` policy to the new role to allow the Lambda function to write to CloudWatch Logs. This is performed with the following CloudWatch command.
```bash 
aws iam attach-role-policy --role-name lambda-demo --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
```

## Deploy 
We can now deploy the lambda function with the following CLI command.
```bash
aws lambda create-function --function-name demo \
  --role <specify role arn from previous step here> \
  --runtime provided --timeout 15 --memory-size 128 \
  --handler demo --zip-file fileb://demo.zip
```

## Test
```bash
aws lambda invoke --function-name demo --cli-binary-format raw-in-base64-out --payload '{"location": "somewhere"}' output.json
```

## References
- [Introducing the C Lambda Runtime](https://aws.amazon.com/blogs/compute/introducing-the-c-lambda-runtime/)
- [C++ Sample Lab](https://github.com/awslabs/aws-lambda-cpp)
- [AWS CLI - Invoke Lambda](https://docs.aws.amazon.com/cli/latest/reference/lambda/invoke.html#examples)
- [AWS CLI - Payload Error](https://stackoverflow.com/questions/60310607/amazon-aws-cli-not-allowing-valid-json-in-payload-parameter)
- [YouTube Walk Through](https://youtu.be/LaDrQqrrmrI)