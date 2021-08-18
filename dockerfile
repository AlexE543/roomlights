FROM continuumio/miniconda

WORKDIR /api

COPY environment.yml environment.yml
COPY entrypoint.sh entrypoint.sh
COPY /api /api

RUN conda env create -f environment.yml

RUN chmod +x entrypoint.sh

RUN echo "source activate roomlights" >> ~/.bashrc
ENV PATH /opt/conda/envs/roomlights/bin:$PATH

EXPOSE 8080

ENTRYPOINT ["./entrypoint.sh"]