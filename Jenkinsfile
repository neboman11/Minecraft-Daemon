pipeline {
  agent any
  stages {
    stage('Setup') {
      steps {
        sh 'git submodule update --init --recursive'
        sh 'mkdir build'
        dir(path: 'build') {
          sh 'cmake CODE_COVERAGE=ON ..'
        }

      }
    }

    stage('Build') {
      steps {
        dir(path: 'build') {
          sh 'make'
        }

      }
    }

    stage('Save Executable') {
      steps {
        archiveArtifacts(artifacts: 'build/src/Minecraft-Daemon', caseSensitive: true)
      }
    }

    stage('Test') {
      steps {
        sh 'ctest'
      }
    }

    stage('Collect Coverage') {
      steps {
        sh 'lcov --directory . --capture --output-file coverage.info'
        sh 'lcov --remove coverage.info \'/usr/*\' "${HOME}"\'/.cache/*\' --output-file coverage.info'
        sh 'lcov --list coverage.info'
        sh 'bash <(curl -s https://codecov.io/bash) -f coverage.info || echo "Codecov did not collect coverage reports"'
      }
    }

  }
  environment {
    CODECOV_TOKEN = '5f5c6ae9-85f2-4a63-8219-69fef3151803'
  }
}