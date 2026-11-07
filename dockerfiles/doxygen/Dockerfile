# Small debian-slim base image (~74MB) avoiding heavy ubuntu:latest images
FROM debian:bookworm-slim AS build

# Pre-install Doxygen and Graphviz in an early layer for fast rebuilds
RUN apt-get update && apt-get install -y --no-install-recommends \
    doxygen \
    graphviz \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . /workspace

# Generate Doxygen HTML documentation (README.md is configured as the main landing page)
RUN doxygen Doxyfile && \
    test -f html/index.html && \
    test -f html/annotated.html

# Optional stage to export generated documentation locally:
#   docker build -f dockerfiles/doxygen/Dockerfile --target export --output html .
FROM scratch AS export
COPY --from=build /workspace/html /

FROM build AS verify
CMD ["echo", "Documentation generated successfully in /workspace/html!"]
