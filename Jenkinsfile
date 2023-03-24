pipeline {
  agent {
    kubernetes {
      inheritFrom 'my-agent-pod'
      yaml """
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: jnlp
    resources:
      limits:
        memory: "1Gi"
        cpu: "500m"
      requests:
        memory: "1Gi"
        cpu: "500m"
  - name: ubuntu-sumo
    image: ghcr.io/eclipse/eclipse-sumo-build-ubuntu:latest
    tty: true
    resources:
      limits:
        memory: "2Gi"
        cpu: "1"
      requests:
        memory: "2Gi"
        cpu: "1"    
    command:
    - cat
    env:
    - name: "MAVEN_OPTS"
      value: "-Duser.home=/home/jenkins"
    volumeMounts:
    - name: settings-xml
      mountPath: /home/jenkins/.m2/settings.xml
      subPath: settings.xml
      readOnly: true
    - name: settings-security-xml
      mountPath: /home/jenkins/.m2/settings-security.xml
      subPath: settings-security.xml
      readOnly: true
    - name: m2-repo
      mountPath: /home/jenkins/.m2/repository
  volumes:
  - name: settings-xml
    secret:
      secretName: m2-secret-dir
      items:
      - key: settings.xml
        path: settings.xml
  - name: settings-security-xml
    secret:
      secretName: m2-secret-dir
      items:
      - key: settings-security.xml
        path: settings-security.xml
  - name: m2-repo
    emptyDir: {}
"""
    }
  }
  stages {
    stage('Building SUMO') {
      steps {
        container('ubuntu-sumo') {
          sh '''
            git fetch --tags --force
            mkdir -p cmake-build 
            cd cmake-build 
            export CC=gcc; export CXX=g++; 
            cmake ..
            make -j2 
          '''
        }
      }
    }
    stage('Build TraaS') {
      steps {
        container('ubuntu-sumo') {
          sh '''
            cd cmake-build
            make traas
          '''
        }
      }
    }
    stage('Maven Artifact - libsumo') {
      steps {
        container('ubuntu-sumo') {
          sh '''
            cd cmake-build/src/libsumo
            mvn --batch-mode deploy
          '''
        }
      }
    }
    stage('Maven Artifact - libtraci') {
      steps {
        container('ubuntu-sumo') {
          sh '''
            cd cmake-build/src/libtraci 
            mvn --batch-mode deploy
          '''
        }
      }
    }
  }
}
