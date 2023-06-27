const uuid = () => crypto.randomUUID();

async function readRequestBody(request) {
  const contentType = request.headers.get("content-type");

  if (contentType.includes("form")) {
    const formData = await request.formData();
    const body = {};

    for (const entry of formData.entries()) {
      body[entry[0]] = entry[1];
    }
    return JSON.stringify(body);
  }
}

export async function onRequest(context) {
  const reqBody = await readRequestBody(context.request);
  if(reqBody.length > 1024){
    return new Response("No me llenes la DB con basura :P");
  }
  await context.env.DB.put(uuid(), reqBody);
  
  return new Response("¡Muchas gracias por inscribirte!");
}