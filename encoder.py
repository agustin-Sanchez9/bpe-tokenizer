import os


# carga las reglas definidas por trainer.py
def load_merges(file_path):
    merges = []
    if not os.path.exists(file_path):
        raise FileNotFoundError
    
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            if line.split():
                pair = tuple(map(int, line.split())) # convertir a tupla los datos de merges.txt
                merges.append(pair)
    return merges


# codifica texto con la lista de tokens
def encode(text, merges):
    tokens = list(text.encode("utf-8"))

    for i, pair in enumerate(merges):
        new_token_id = 256 + i
        new_tokens = []
        j = 0
        while j < len(tokens):
            if j < len(tokens) - 1 and tokens[j] == pair[0] and tokens[j+1] == pair[1]:
                new_tokens.append(new_token_id)
                j += 2
            else:
                new_tokens.append(tokens[j])
                j += 1
        tokens = new_tokens
    
    return tokens


# convierte los tokens devuelta a texto legible
def decode(tokens, merges):
    vocab = {i: bytes([i]) for i in range(256)} # vocabulario base (0-255)

    # reconstruir el vocabulario basado en los merges
    for i, (p1, p2) in enumerate(merges):
        vocab[256 + i] = vocab[p1] + vocab[p2]

    result_bytes = b"".join(vocab[idx] for idx in tokens) # concatenar los bytes de cada token

    return result_bytes.decode("utf-8", errors="replace")


# devuelve la lista de string que representa cada token
def debug_tokens(tokens, merges):
    vocab = {i: bytes([i]) for i in range(256)}
    for i, (p1, p2) in enumerate(merges):
        vocab[256 + i] = vocab[p1] + vocab[p2]
    

    visual_list = []
    for t in tokens:
        byte_value = vocab[t]
        try:
            char_value = byte_value.decode('utf-8')
        except UnicodeDecodeError:
            char_value = str(list(byte_value)) # Muestra los bytes si no es texto completo
            
        visual_list.append(f"'{char_value}'")
    
    return visual_list



# Devuelve la representacion en caracteres de un ID de token especifico
def get_token_content(token_id, merges):
    # vocabulario base
    vocab = {i: bytes([i]) for i in range(256)}
    
    # reconstruir los tokens fusionados hasta llegar al token objetivo
    for i, (p1, p2) in enumerate(merges):
        current_id = 256 + i
        vocab[current_id] = vocab[p1] + vocab[p2]
        if current_id == token_id:
            break
            
    if token_id not in vocab:
        return f"Error: El token ID {token_id} no existe en las reglas actuales."

    byte_value = vocab[token_id]
    
    # Intentar decodificar a texto, si no es posible (bytes incompletos), mostrar los bytes raw
    try:
        char_value = byte_value.decode('utf-8')
        return f"Token {token_id} -> '{char_value}'"
    except UnicodeDecodeError:
        return f"Token {token_id} -> {list(byte_value)} (Bytes parciales de UTF-8)"




if __name__ == "__main__":
    try:
        rules = load_merges("merges.txt")
        print(f"Se cargaron {len(rules)} reglas.")

        input_text = "el Quijote y Sancho"
        input_token = 4967

        tokens = encode(input_text, rules)
        visual_tokents = debug_tokens(tokens, rules)
        debug_token = get_token_content(input_token, rules)

        print(f"\nTexto original: {input_text}")
        print(f"Tokens generados: {tokens}")
        print(f"Desglose de tokens: [{' | '.join(visual_tokents)}]")
        print(f"Longitud original (bytes): {len(input_text.encode('utf-8'))}")
        print(f"Longitud comprimida (tokens): {len(tokens)}")
        decoded_text = decode(tokens, rules)
        print(f"\nTexto recuperado: {decoded_text}")
        print("\n")
        print(f"El token {input_token} representa: {debug_token}")
        print("\n")

    except Exception as e:
        print(f"Error: {e}")