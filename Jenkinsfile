pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        sh '/usr/lib/go-1.15/bin/go build'
      }
    }

    stage('Save Executable') {
      steps {
        archiveArtifacts(artifacts: 'Minecraft-Daemon', caseSensitive: true)
      }
    }

    stage('Test') {
      steps {
        dir(path: 'build') {
          sh '/usr/lib/go-1.15/bin/go test || true'
        }

      }
    }

    stage('Collect Coverage') {
      steps {
        sh './get_code_cov.sh'
        sh 'curl -s https://codecov.io/bash | bash'
      }
    }

  }
  environment {
    CODECOV_TOKEN = '5f5c6ae9-85f2-4a63-8219-69fef3151803'
  }
}